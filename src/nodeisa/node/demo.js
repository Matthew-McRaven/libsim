const nodeisa = require('./nodeisa.js');

function sleep(ms) {
	return new Promise((resolve) => {
	  setTimeout(resolve, ms);
	});
  }   

nodeisa().then(function(instance) {
	var x = new instance.Application();
	x.SayHello()
});;