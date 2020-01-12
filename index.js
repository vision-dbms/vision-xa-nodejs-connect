function usingExternalSessionConfig () {
    return process.env.VcaGlobalSessionsFile
        || process.env.VcaSessionsFile;
}

function usingExternalStartupExpression () {
    return process.env.VisionStartExpr;
}

const ThePrimitives = (
    ()=>{
        if (!usingExternalSessionConfig () && !usingExternalStartupExpression ()) {
            process.env.VcaSessionsFile = require.resolve ('./resources/session.cfg');
            process.env.VisionStartExpr = `"${require.resolve('./resources/node.vis')}" asFileContents evaluate`;
        }
//        return Promise.resolve (require ('./build/Debug/vxanode'));
        return Promise.resolve (require ('./build/Release/vxanode'));
    }
) ();

function PrimitiveOperationPromise (f/*(thePrimitives,resolvefn,rejectfn)*/) {
    return new Promise (
        (resolve,reject)=>ThePrimitives.then (
            thePrimitives=>f(thePrimitives,resolve,reject),
            theUnexpected=>reject (theUnexpected)
        )
    );
}

function evaluate (...args) {
    return PrimitiveOperationPromise (
        (thePrimitives,resolve,reject)=>thePrimitives.v(
            msg=>reject(new Error(msg)),resolve,...args
        )
    );
}

function offer (...args) {
    return PrimitiveOperationPromise (
        (thePrimitives,resolve,reject)=>resolve(thePrimitives.o(...args))
    );
}

function register (...args) {
    return new Promise (
        (resolve,reject)=>{
            registry ().then (
                registryObject=>resolve(Object.assign (registryObject, ...args))
            )
        }
    );
}

function registry (...args) {
    return PrimitiveOperationPromise (
        (thePrimitives,resolve,reject)=>resolve(thePrimitives.registry(...args))
    );
}

function shutdown (...args) {
    return PrimitiveOperationPromise (
        (thePrimitives,resolve,reject)=>resolve(thePrimitives.shutdown(...args))
    );
}

module.exports.evaluate = module.exports.v = evaluate;
module.exports.offer    = module.exports.o = offer;
module.exports.register = register;
module.exports.registry = registry;
module.exports.shutdown = shutdown;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

register ({
    awaitHelper: function (awaited, task) {
        task.suspend ();
        (async function () {
            try {
                await awaited;
            } catch (e) {
            }
            task.resume ();
        })();
        return awaited;
    }
});
