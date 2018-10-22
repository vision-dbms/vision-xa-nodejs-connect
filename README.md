[node.js](https://nodejs.org) native addon connectivity to the [Vision](https://github.com/vision-dbms/vision) database management system.
```js
var vx=require('@vision-dbms/connect')
var p=vx.v ('2 + 2'). then (r=>console.log (r))

>     4.00


var myObject = {}
var p=vx.v ('JS set: "x" to: 23.7; JS set: "y" to: "Hello, world..."',myObject)
var t=p.then (r=>console.log (myObject))

> { x: 23.7, y: 'Hello, world...' }
```

## Installation

This [node](https://nodejs.org) native module is available from the [npm registry](https://npmjs.com/package/@vision-dbms/connect) as _[@vision-dbms/connect](https://npmjs.com/package/@vision-dbms/connect)_.
