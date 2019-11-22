#!/usr/bin/env node

const vc = require ('./index');
const fetch = require ('node-fetch');

/********************
 *****  Logger  *****
 ********************/
const winston = require('winston');

const logger = winston.createLogger({
  level: 'info',
  format: winston.format.json(),
  defaultMeta: { service: 'vision-fetch' },
  transports: [
    //
    // - Write to all logs with level `info` and below to `combined.log`
    // - Write all logs error (and below) to `error.log`.
    //
    new winston.transports.File({ filename: 'error.log', level: 'error' }),
    new winston.transports.File({ filename: 'combined.log' })
  ]
});

//
// If we're not in production then log to the `console` with the format:
// `${info.level}: ${info.message} JSON.stringify({ ...rest }) `
//
if (process.env.NODE_ENV !== 'production') {
  logger.add(new winston.transports.Console({
    format: winston.format.simple()
  }));
}

/********************************
 *****  Global Definitions  *****
 *                              *
 * Maybe create an official set *
 * of Vision global definitions *
 * as part of the first release *
 *                              *
 ********************************/

global.New = function New (constructable, ...constructorArgs) {
    return new constructable (...constructorArgs);
}

/*****************************
 *****  Promise Helpers  *****
 *****************************/

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

/***************************
 *****  Shared Object  *****
 ***************************/

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
        logger.log ({ level: 'info', url, options});
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

/*************************************
 *****  Vca Command Line Access  *****
 *************************************/

var yargs=require('yargs');
//logger.log ({ level: 'info', yargv: yargs.argv});
logger.log ({ level: 'info', yargv: yargs.argv});

var p = vc.o (
    (runState,activityCount,activeOfferCount,passiveOfferCount,listenerCount,...rest)=>{
        logger.log({
            level: 'info', runState, activityCount,activeOfferCount,passiveOfferCount,listenerCount,rest
        });
        if (runState == "Stopped") {
            vc.shutdown ().then(
                r=>logger.log({
                    level: 'info',
                    shutdownCalled: true,
                    vc_stop: String(vc.shutdown)
                }),
                e=>logger.log({
                    level: 'error',
                    shutdownCalled: false
                })
            );
        }
    },so,...yargs.argv._
).then (
    r=>{logger.log({ level: 'info' , message: r})},
    e=>{logger.log({ level: 'error', error: e})}
);
