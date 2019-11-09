const vc = require ('./index');
module.exports.vc = vc;

const fetch = require ('node-fetch');

/****************/
class PromiseWrapper {
    constructor (wrappedPromise) {
        this.wrappedPromise = wrappedPromise;
        wrappedPromise.catch (rejection=>{})
    }
    then (resolve,reject) {
        return reject
            ? this.wrappedPromise.then (resolve,reject)
            : new PromiseWrapper (this.wrappedPromise.then (resolve));
    }
    catch (reject) {
        return this.wrappedPromise.catch (reject);
    }
}
module.exports.PromiseWrapper = PromiseWrapper;
module.exports.WrappedPromise = wrappedPromise=>new PromiseWrapper(wrappedPromise);

/****************/
class PromisedResult {
    constructor (promise) {
        this.promise = promise;
        this.status = 'pending';
        promise.then (
            result=>{
                this.status = 'success';
                this.result = result;
            }, rejection=>{
                this.status = 'reject';
                this.result = rejection;
            }
        );
    }
    json () {
        return this.then (result=>result.json());
    }
    text () {
        return this.then (result=>result.text());
    }
    then (...callbacks) {
        return new PromisedResult (this.promise.then (...callbacks));
    }
}

/****************/
const so = {
    /*----------------*/
    fetch (body,url) {
        return new PromisedResult (
            fetch (
                url, {
                    method: 'post',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify (body)
                }
            )
        );
    },
    fetchText (body,url) {
        return this.fetch (body,url).text ();
    },
    fetchJSON (body,url) {
        return this.fetch (body,url).json ();
    },

    /*----------------*/
    fetchFrom (url,options) {
        console.log (url, options);
        return new PromisedResult (
            fetch (url,options)
        );
    },
    fetchTextFrom (url,options) {
        return this.fetchFrom (url,options).text ();
    },
    fetchJSONFrom (url,options) {
        return this.fetchFrom (url,options).json ();
    }
}
module.exports.so = so;

/****************/
var p = vc.o (so,'-address=2300').then (
    r=>{console.log (r); module.exports.st=r}
);
