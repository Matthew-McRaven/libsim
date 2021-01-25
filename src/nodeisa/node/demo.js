const pep10 = require('./pep10.js');
const pep9 = require('./pep9.js');
const sim = require('./sim.js');

pep10().then(function(instance) {
	var def = new instance.IsaDefinition
	//console.log(def.map)
	//get_bit = (index) =>{return instance.WildEyes.get_bit(def.isa, index)}
	console.log(def.get_instruction(instance.InstructionMnemonic.ADDX))

});

module.exports = [pep9, pep10, sim]