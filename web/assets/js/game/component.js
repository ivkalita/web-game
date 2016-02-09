define(['./scene'], function(Scene) {

	var Component = function(scene) {
		this.scene = scene;
		this.gl = scene.gl;
	};

	Component.prototype.update = function(delta) { };
	Component.prototype.render = function() { };

	return Component;
});