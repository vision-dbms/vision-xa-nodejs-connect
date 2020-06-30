# Connecting Vision to External Environments

This document describes how to incorporate a <b>Node.js</b> module to
connect Vision with external environments.  <b>Node.js</b> is an
open-source, cross-platform, JavaScript runtime environment.
<b>Node.js</b> allows unifying web-application development
around a single programming language, rather than different languages
for server- and client-side scripts.

The instructions for installing the [node](https://nodejs.org) module
are available in the [README](README.md).  This document describes the
starter protocol provided to interact with JSON objects from Vision from two
perspectives:

* Running a url from Vision and retrieving values from a returned JSON
  object.  We will refer to this usage as XXX for now.

* Using Vision to create JSON data bundles which can be invoked from
  non-Vision clients such as Python. We will refer to this usage as
  YYY for now.



##  Interactive js example

{whatever that means - show how to do basics outlined in README}

*  XXX quick start
*  YYY quick start


##  The __node.vis__ file and the <b>JS</b> Class

The <i>node.vis</i> file contains the protocol to create the Vision <b>JS</b>
class designed to interact with the nodejs module.  The methods are written in
Vision, but are highly stylized in a way that interfaces with the JS
langauge and JSON objects.  (For those of you who can't read between the lines, that means that Mike named and implemented thes methods :-) ).
The class and its methods are included with the __nodejs__ downloaded from git.

### Open an External Connection

To start, you need to create a connection to an external object using
the __serverByName:__ message defined at the <b>JS</b> class.

> !jsConnect <- JS serverByName: "Node_Fetch" ;

The parameter to this message references an entry in the
<i>sessions.cfg</i> file in the form:

<pre>
Connection_Template Begin
Name Node_Fetch
Program $$(DummyLocation)/vision-xa-nodejs-connect/setup.js
Option plumbed
Connection_Template End
</pre>

When you run this expression, you should see a __node__ subprocess
created and owned by your current __batchvision__ session.

### Building JSON Objects

You can create an object in Vision that can be supplied as a JSON
object to an external environment.  JSON objects are written in key/value pairs.
Keys must be strings, and values can be numbers, strings, arrays,
or other JSON objects.

To create a new object use:

> !jsonObject <- jsConnect newObject ;

<b>jsonObject</b> represents a basic JSON object.  To add simple
key/value pairs to this object, use:

> jsonObject
>    set: "keyString" to: "this is a string" .
>    set: "keyNumber" to: 123.45 .
>  ;

To look at the key/value pairs in this object use:

> jsonObject display;

You can access an individual value in this object using the <i>get:</i>
message.  The parameter to <i>get:</i> can be supplied as either a
string or block and names the key you want to retrieve:


> jsonObject get: "keyString"

or

> jsonObject get: [ keyString ]

For simple navigations, the string form is often preferred.
You will see later examples where the advantages of using the block
form become more obvious.

You can create and name a reference to a JSON array using:

> jsonObject <br>
>    set: "ids" toArrayFrom: ("FDS", "IBM", "MSFT") ; <br>
> jsonObject display ; <br>

Notice that the display prints values for atomic data types like
number and string.  Since __ids__ references an external array object, you will just
see an indication that the value is __JS__.  The <i>get:</i> message can
be used to navigate to a struture within the JSON object and display
its content.

>  jsonObject get: "ids" . display ;

or

> jsonObject get: [ ids ] . display ;

Since the __ids__ object is a JSON array, the displayed output is an
enumerated list of position/value pairs.  You can look at a specific
element in this array using the __get:__ message with an integer value
as its parameter.  Note that the array is indexed starting at 0.  To
see the first element, use:

> jsonObject get: "ids" . get: 0

One advantage of using the block form for the __get:__ parameter is
that the entire library of JS functions are available as operations.
For example, to get the length of the array and display each element, use:

> !ids <- jsonObject get: "ids" ; <br>
> !length <- ids get: [ length ] ;   #<-- this is a JS function <br>
> length sequence0 <br>
> do: [ print ; ^my ids get: ^self . printNL ] ; <br>


JSON objects can reference other JSON objects.  For example:

> !idInputs <- jsConnect newObject <br>
>    set: "type" to: "Security" . <br>
>    set: "ids" toArrayFrom: ("FDS", "IBM", "MSFT") ; <br>

> !settingInputs <- jsConnect newObject <br>
>    set: "curr" to: "USD" . <br>
>    set: "adjust" to: "NO" ; <br>

> !requestObject <- jsConnect newObject <br>
>    set: "inputs" to: idInputs . <br>
>    set: "settings" to: settingInputs ; <br>

> "--- Display requestObject: " printNL ; <br>
> requestObject display ; <br>

> "--- Display requestObject settings: " printNL ; <br>
> requestObject get: "settings" . display ; <br>


You can navigate the JSON object using multiple __get:__ messages:

> requestObject get: "settings" . get: "adjust"

Alternatively, messages can be applied within a single block:

> requestObject get: [ settings adjust ]

The __at:__ message can be used to reference a JSON array element from within a block.
For example, both of the expressions below can be used to find the second identifier:

> requestObject get: "inputs" . get: "ids" . get: 1 . printNL ; <br>

> requestObject get: [ inputs ids at: 1 ] . printNL ; 



### Defining and Running a URL

*  calling the service
*  GET
*  POST
*  Text Reponse
*  JSON Object Response

###  Navigating the JSON Object Response


##  Samples

###  Sample Vision Calling an API

###  Sample Calling Vision from Jupyter Notebook
