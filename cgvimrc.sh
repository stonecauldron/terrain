#!/bin/sh

executable=$1
cd "$executable"
echo "set makeprg=make\ -C\ ../build" > .vimrc
echo "nnoremap <F4> :make!<cr>" >> .vimrc
echo "nnoremap <F5> :!cd ../build/${executable};./${executable}<cr>" >> .vimrc
