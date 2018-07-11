const primitive = require ('./build/Release/vxanode');

module.exports.primitive = primitive;

module.exports.v = function (...args) {
    var prodCount=0;
    const prodId=setInterval(()=>{prodCount++},30);
    const stopProdding=r=>{
        clearInterval(prodId);
        return r;
    }
    return primitive.v(...args).then (
        response=>stopProdding(response),
        response=>stopProdding(response)
    );
}
