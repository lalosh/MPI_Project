var spawn = require('child_process').spawn;
var prc = spawn('./testTime',[]);

//noinspection JSUnresolvedFunction
var result ="";

prc.stdout.setEncoding('utf8');

prc.stdout.on('data', function (data) {
    result += data.toString();
});


prc.stdout.on('finish',function(){
  console.log(result);
});
