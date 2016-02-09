define(function() {
	
	var FpsMeter = function() {
		
		var time0 = null;
		var frames = 0;
		this.fps = 0;
		
		this.start = function() {
			this.time0 = Date.now();
			this.frames = 0;
		};
		
		this.update = function() {
			frames++;
			var time = Date.now();
			var delta = time - time0;
			if (delta >= 1000) {
				this.fps = frames * 1000 / delta;
				frames = 0;
				time0 = time;
			}
		};
	};
	
	return FpsMeter;
});