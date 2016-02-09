define(['./asset-manager'], function() {

	return {
		load: function(assetManager) {
			return assetManager.load({
				textures: {
					sky: {
						layer0: '/assets/images/sky/layer0.png',
						layer1: '/assets/images/sky/layer1.png',
						layer2: '/assets/images/sky/layer2.png',
						layer3: '/assets/images/sky/layer3.png',
					},
					planetTemplate: '/assets/images/planet-template.png'
				},
				programs: {
					skyLayer: '/assets/shaders/sky-layer.program',
					planet: '/assets/shaders/planet.program'
				}
			});
		}
	};

});