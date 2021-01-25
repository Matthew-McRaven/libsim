const pep10 = require('./pep10.js');
const pep9 = require('./pep9.js');
const sim = require('./sim.js');

pep10().then(function(instance) {
	var def = new instance.IsaDefinition
	//console.log(def.map)
	//get_bit = (index) =>{return instance.WildEyes.get_bit(def.isa, index)}
	console.log(def.isa.get(instance.InstructionMnemonic.ADDX))
	// Prove that we can iterate over all mnemonics, and fetch the associated ISA definition.
	for (const [key, value] of Object.entries(instance.InstructionMnemonic.values)) {
		console.log(def.isa.get(value))
	}

});

module.exports = [pep9, pep10, sim]