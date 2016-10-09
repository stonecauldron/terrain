#!/usr/bin/env coffee

nb = process.argv[2]
angles = (Math.random() * 360 for _ in [0 .. (nb - 1)])
vectors = ([Math.cos(angle), Math.sin(angle), 0] for angle in angles)
console.log vectors.join ', '
