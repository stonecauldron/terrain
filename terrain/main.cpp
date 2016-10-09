#include "icg_common.h"
#include "FrameBuffer.h"
#include "_grid/Grid.h"
#include "_perlin/PerlinQuad.h"
#include "_skybox/Skybox.h"
#include "_point/Point.h"
#include "_bezier/Bezier.h"

#define GRID_WIDTH 1024

int width=1280, height=720;
int grid_width = GRID_WIDTH;

FrameBuffer fb(grid_width, grid_width);

FrameBuffer fb_mirror(width, height);

PerlinQuad perlin;
Grid grid;
Grid water;
Skybox skybox;

BezierCurve cam_pos_curve;
BezierCurve cam_look_curve;
std::vector<ControlPoint> cam_pos_points;
std::vector<ControlPoint> cam_look_points;

GLfloat height_map[GRID_WIDTH * GRID_WIDTH];

vec3 cam_pos(0.0f, 0.2f, 3.0f);
vec3 cam_up(0.0f, 1.0f, 0.0f);
vec3 cam_front(0.0f, 0.0f, -1.0f);
vec3 mirror_cam_pos(0.5f, -0.1f, 0.5f);
vec3 mirror_cam_up(0.0f, 1.0f, 0.0f);
vec3 mirror_cam_front(0.0f, 0.0f, -1.0f);



vec3 cam_speed(0.0f, 0.0f, 0.0f);

bool keys[1024];
enum Camera_mode {FREE, FPS, BEZIER};
Camera_mode cam_mode = FREE;

void fill_height_map(GLuint texture) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, (void*)height_map);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void init_cam_pos_curve();
void init_cam_look_curve();

void init(){
    glClearColor(1,1,1, /*solid*/1.0 );
    glEnable(GL_DEPTH_TEST);
    GLuint fb_tex = fb.init();
    GLuint mirror_tex = fb_mirror.init(false, true);
    grid.init(grid_width, fb_tex, mirror_tex, "_grid/grid_vshader.glsl", "_grid/grid_fshader.glsl");
    water.init(grid_width, fb_tex, mirror_tex, "_grid/water_vshader.glsl", "_grid/water_fshader.glsl");
    perlin.init();
    skybox.init();

    init_cam_pos_curve();

    init_cam_look_curve();

    ///--- Render to FB
    fb.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        perlin.draw();
    fb.unbind();
    // fb.display_color_attachment("FB - Color"); ///< debug

    // fill height_map
    fill_height_map(fb_tex);
}

GLfloat get_height(GLint x, GLint y) {
  GLfloat offset = 0.2f;
  if (x < 0 || x > GRID_WIDTH || y < 0 || y > GRID_WIDTH) {
    return 0.0f;
  }
  GLint index = GRID_WIDTH * (y) + x;
  return height_map[index] + offset;
}

void camera_movement() {
  GLfloat speed_increment = 0.01f;
  GLfloat rotation_increment = M_PI/512.0f;

  // friction factor
  GLfloat mu = 0.2f;

  // pitch matrix
  typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
  Transform pitch = Transform::Identity();
  Transform mirror_pitch = Transform::Identity();
  vec3 cam_x = cam_up.cross(cam_front);
  pitch *= Eigen::AngleAxisf(cam_speed.y(), cam_x);
  mirror_pitch *= Eigen::AngleAxisf(-cam_speed.y(), cam_x);

  Transform yaw = Transform::Identity();
  yaw *= Eigen::AngleAxisf(cam_speed.z(), cam_up);

  if (keys['W']) {
    cam_speed.x() += speed_increment;
  }
  if (keys['S']) {
    cam_speed.x() -= speed_increment;
  }
  if (keys['Q']) {
    cam_speed.y() += rotation_increment;
  }
  if (keys['E']) {
    cam_speed.y() -= rotation_increment;
  }
  if (keys['A']) {
    cam_speed.z() += rotation_increment;
  }
  if (keys['D']) {
    cam_speed.z() -= rotation_increment;
  }
  // compute friction
  GLfloat x_friction = -cam_speed.x() * mu;
  cam_speed.x() += x_friction;
  GLfloat y_friction = -cam_speed.y() * mu;
  cam_speed.y() += y_friction;
  GLfloat z_friction = -cam_speed.z() * mu;
  cam_speed.z() += z_friction;

  // update velocity of camera
  cam_pos += cam_speed.x() * cam_front;
  cam_front = pitch * cam_front;
  cam_up = pitch * cam_up;
  mirror_cam_up = mirror_pitch * mirror_cam_up;
  cam_front = yaw * cam_front;
}

void snap_to_terrain() {
  GLint grid_x = (cam_pos.x() + 1)/2.0f * GRID_WIDTH;
  GLint grid_z = (cam_pos.z() + 1)/2.0f * GRID_WIDTH;
  cam_pos.y() = get_height(grid_x, grid_z);
}

void check_camera_mode() {
  if (keys['0']) {
    cam_mode = FREE;
    std::cout << "Free camera mode activated." << std::endl;
    cam_pos = vec3(0, 0.2, 3);
  }
  if (keys['1']) {
    cam_mode = FPS;
    std::cout << "FPS camera mode activated." << std::endl;
    cam_pos = vec3(0, 0.2, 3);
  }
  if (keys['2']) {
    cam_mode = BEZIER;
    std::cout << "Bezier camera mode activated." << std::endl;
  }
}

void display() {
    check_camera_mode();
    if (cam_mode != BEZIER) {
        camera_movement();
    }
    if (cam_mode == FPS) {
      snap_to_terrain();
    }

    opengp::update_title_fps("FrameBuffer");
    glViewport(0,0,width,height);

    ///--- Setup view-projection matrix
    float ratio = width / (float) height;
    static mat4 projection = Eigen::perspective(45.0f, ratio, 0.1f, 50.0f);
    vec3 cam_look = cam_pos + cam_front;
    mat4 view = Eigen::lookAt(cam_pos, cam_look, cam_up);
    
    if (cam_mode == BEZIER) {
        float t = (sin(glfwGetTime() * 1/7.5) + 1) / 2.0;
        cam_pos_curve.sample_point(t, cam_pos);
        cam_look_curve.sample_point(t, cam_look);
        view = Eigen::lookAt(cam_pos, cam_look, cam_up);
    }

    mat4 VP = projection * view;

    vec3 water_normal(0.0f, -1.0f, 0.0f);
    vec3 u = water_normal * (cam_pos.dot(water_normal));
    vec3 mirror_cam_pos = vec3(cam_pos.x(), -cam_pos.y(), cam_pos.z());
    vec3 mirror_cam_look = vec3(cam_look.x(), -cam_look.y(), cam_look.z());
    mat4 mirror_view = Eigen::lookAt(mirror_cam_pos, mirror_cam_look, mirror_cam_up);
    mat4 mirror_VP = projection * mirror_view;


    



    ///--- Render to Window
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.draw(view, projection);
    grid.draw(VP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    water.draw(VP);
    glDisable(GL_BLEND);


    // water becomes lava
    fb_mirror.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.draw(mirror_view, projection);
        glEnable(GL_CLIP_PLANE0);
        grid.draw(mirror_VP);
        glDisable(GL_CLIP_PLANE0);

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDisable(GL_BLEND);*/
    fb_mirror.unbind();
}

void keyboard(int key, int action) {
  if (action == GLFW_PRESS) {
    keys[key] = true;
  } else if (action == GLFW_RELEASE) {
    keys[key] = false;
  }
}

void init_cam_look_curve() {
    cam_look_curve.init();
    cam_look_points.push_back(ControlPoint(0.5, 0.1, 0.5, 7));
    cam_look_points.push_back(ControlPoint(0, 0.1, 0.5, 8));
    cam_look_points.push_back(ControlPoint(-0.5, 0.1, 0.0, 9));
    cam_look_points.push_back(ControlPoint(0, 0.1, -0.5, 10));
    cam_look_points.push_back(ControlPoint(0.7, 0.1, 0.16, 11));
    cam_look_points.push_back(ControlPoint(0.21, 0.1, 0.26, 12));
    cam_look_points.push_back(ControlPoint(0.5, 0.1, 0.8, 13));
    for (unsigned int i = 0; i < cam_look_points.size(); i++) {
        cam_look_points[i].init();
    }

    cam_look_curve.set_points(cam_look_points[0].position(), cam_look_points[1].position(), cam_look_points[2].position(), cam_look_points[3].position());
    cam_look_curve.add_segment(cam_look_points[4].position(), cam_look_points[5].position(), cam_look_points[6].position());
}

void init_cam_pos_curve() {
    cam_pos_curve.init();
    cam_pos_points.push_back(ControlPoint(0.5, 0.1, 1.0, 0));
    cam_pos_points.push_back(ControlPoint(0, 0.12, 1.5, 1));
    cam_pos_points.push_back(ControlPoint(-3.5, 2.5, -1.5, 2));
    cam_pos_points.push_back(ControlPoint(-1.5, 0.85, -1.5, 3));
    cam_pos_points.push_back(ControlPoint(0, 2.2, -1.5, 4));
    cam_pos_points.push_back(ControlPoint(1.5, 0.25, 0, 5));
    cam_pos_points.push_back(ControlPoint(1.5, 0.3, 1.5, 6));
    for (unsigned int i = 0; i < cam_pos_points.size(); i++) {
        cam_pos_points[i].init();
    }

    cam_pos_curve.set_points(cam_pos_points[0].position(), cam_pos_points[1].position(), cam_pos_points[2].position(), cam_pos_points[3].position());
    cam_pos_curve.add_segment(cam_pos_points[4].position(), cam_pos_points[5].position(), cam_pos_points[6].position());
}

int main(int, char**){
    glfwInitWindowSize(width, height);
    glfwCreateWindow();
    glfwDisplayFunc(display);
    glfwSetKeyCallback(keyboard);
    init();
    keyboard(GLFW_KEY_KP_1, 0);
    //glfwSwapInterval(0); ///< disable VSYNC (allows framerate>30)
    glfwMainLoop();
    return EXIT_SUCCESS;
}
