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
        return Promise.resolve (require ('./build/Release/vxanode'));
    }
) ();

function PrimitiveOperationPromise (f) {
    return new Promise (
        (resolve,reject)=>ThePrimitives.then (
            thePrimitives=>f(thePrimitives,resolve,reject),
            theUnexpected=>reject (theUnexpected)
        )
    );
}

module.exports.v = function (...args) {
    return PrimitiveOperationPromise (
        (thePrimitives,resolve,reject)=>thePrimitives.v2(reject,resolve,...args)
    );
}

module.exports.o = function (...args) {
    return PrimitiveOperationPromise (
        (thePrimitives,resolve,reject)=>resolve(thePrimitives.o2(s=>{console.log(s)},...args))
    );
}
