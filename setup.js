// const log = require('why-is-node-running') // should be your first require

const vc = require ('./index');
module.exports.vc = vc;

const fetch = require ('node-fetch');

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

class PromisedResult {
    constructor (consumedPromise) {
        this.fulfillment = 'pending';
        this.result;
        consumedPromise.then (
            result=>{
                this.fulfillment = 'success'
                this.result = result;
            }, rejection=>{
                this.fulfillment = 'reject'
                this.result = rejection;
            }
        );
    }
}

const so = {
    fetch (url,body) {
        return new PromiseWrapper (
            fetch (
                url, {
                    method: 'post',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify (body)
                }
            )
        );
    },

    fetchText (url,body) {
        return new PromisedResult (
            this.fetch (url,body).then (result=>result.text ())
        );
    },

    fetchJSON (url,body) {
        return new PromisedResult (
            this.fetch (url,body).then (result=>result.json ())
        );
    }
}
module.exports.so = so;

var p = vc.v('2 + 2').then (
    r=>{
	console.log(r);
	vc.o (so,'-address=2300').then (r=>module.exports.st=r)
    }
);
// var p = vc.o (so,'-address=2300').then (res=>module.exports.st=res);
