### _A [node.js](https://nodejs.org) native add-on for the [Vision](https://github.com/vision-dbms/vision) database management system:_
```js
var vc=require('@vision-dbms/connect')
var p=vc.v ('2 + 2'). then (r=>console.log (r))

>     4.00


var myObject = {}
var p=vc.v ('JS set: "x" to: 23.7; JS set: "y" to: "Hello, world..."',myObject)
var t=p.then (r=>console.log (myObject))

> { x: 23.7, y: 'Hello, world...' }
```

## Prerequisites

The documentation that follows assumes:

- a reasonably modern LTS version of [node](https://nodejs.org/en/download/) (the 8.x track or newer is recommended).
- a version of Vision built from the _[staging-nodejs-connect](https://github.com/vision-dbms/vision/tree/staging-nodejs-connect)_ branch of [Vision's github repository](https://github.com/vision-dbms/vision/tree/staging-nodejs-connect).
- access to one of more Vision databases.

## Installation

This [node](https://nodejs.org) module is available from the [npm registry](https://npmjs.com/package/@vision-dbms/connect) as _[@vision-dbms/connect](https://npmjs.com/package/@vision-dbms/connect)_:

```bash
npm install @vision-dbms/connect
```

Because _[@vision-dbms/connect](https://npmjs.com/package/@vision-dbms/connect)_ is a _native_ add-on for node, it contains C++ code that must be compiled.  ```npm install``` does that automatically, provided the required development tools are present on the system.  If that system is a _Linux_ or _macOS_ host that has already been used to build _Vision_ (perhaps the _[staging-nodejs-connect](https://github.com/vision-dbms/vision/tree/staging-nodejs-connect)_ branch mentioned above), the required tools and header files will already be present.

If the system you are using has not been used to build _Vision_, you may need to install the required development tools and, in the case of _Linux_ systems, some additional system header files. An authoritative list of the tools you need to install can be found in the documentation for _[node-gyp](https://www.npmjs.com/package/node-gyp#installation)_ (note that you do _not_ need to install _[node-gyp](https://www.npmjs.com/package/node-gyp)_, just its recommendations).

For _Linux_ systems, you may also need to install header files for the system's _UUID_ generator.  For Red Hat based systems (RHEL, Centos, etc.), the command you most likely will need to run is:

```bash
sudo yum install libuuid-devel
```

while on Debian based systems (Debian, Ubuntu, etc.), that command will most likely be:


```bash
sudo apt install uuid-dev
```

## Use

### The Basics
To access _@vision-dbms/connect_, load it using _require_:

```js
var vc=require('@vision-dbms/connect')
```

At its most basic, _@vision-dbms/connect_ gives you the ability to execute Vision expressions using its _v_ method:

```js
var p = vc.v ('2 + 2')
```

_v_ operates asynchronously, scheduling evaluation of its expression, returning a JavaScript _Promise_ for the output of that expression.

_Promises_ are a major feature and paradigm of JavaScript programming.  They are described in great depth on the web ([here, for example](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise)).  What you need to know to get started is that _Promises_ have a _then_ method that takes two functions as arguments, one that will be called with a successfully returned result and one that will be called in case of an error:


```js
var p = vc.v ('2 + 2').then(result=>console.log ('Success: ', result), error=>console.error('Failure: ', error))
> Success:       4.00
```

A bit of noteworthy magic is happening here.  The _v_ method knows where to evaluate its expression courtesy of Vision's session configuration files. Found at common user specific and environment variable (e.g., _VcaGlobalSessionsFile_, _VcaSessionsFile_) specified locations, those files are used to create a directory of known servers, services, and process creation templates.  By default, _@vision-dbms/connect_ searches that session directory for an entry named _NodeEvaluator_. It also includes, for its own use in environments that do not define their own session configuration, a very basic _NodeEvaluator_ definition that expects to find a runnable _batchvision_ on the current path:

<pre>
Connection_Template Begin
Name NodeEvaluator
Program batchvision
Connection_Template End
</pre>

In a bare environment, if you do nothing else except make sure that _batchvision_ can be found along with a Vision database it can use, the examples documented here should work.

