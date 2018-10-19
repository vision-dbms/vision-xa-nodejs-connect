A [node](http://nodejs.org) native addon for [Vision](https://github.com/vision-dbms/vision):
```js
var vx=require('@vision-dbms/connect')

var p1=vx.v('2 + 2'). then (r=>console.log (r))
#     4.00


myObject = {}

var p2=vx.v ('JS set: "x" to: 23.7; JS set: "y" to: "Hello, world..."',myObject)

var s=p2.then (r=>console.log (myObject))

# { x: 23.7, y: 'Hello, world...' }
```
