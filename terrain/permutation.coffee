#!/usr/bin/env coffee

shuffle = (a) ->
    i = a.length
    if i is 0 then return false

    while --i
        j = Math.floor(Math.random() * (i+1))
        [a[i], a[j]] = [a[j], a[i]]
    a

max = process.argv[2]
list = [0..max]
list = shuffle list
list = list.join ", "
console.log list
