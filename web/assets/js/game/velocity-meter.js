define(function() {

	var VelocityMeter = function() {
		var MEASURE_TIME_INTERVAL = 0.15;

		var samples = [];

		var filterSamples = function(now) {
			samples = samples.filter(function(sample) {
				return now - sample.time <= MEASURE_TIME_INTERVAL * 1000;
			});
		};

		this.addSample = function(x, y) {
			var now = Date.now();
			samples.push({
				x: x,
				y: y,
				time: now
			});
			filterSamples(now);
		};

		this.calcVelocity = function() {
			filterSamples(Date.now());
			if (samples.length < 2) {
				return {
					x: 0,
					y: 0
				};
			}
			var sample0 = samples[0];
			var sample1 = samples[samples.length - 1];
			var result = {
				x: sample1.x - sample0.x,
				y: sample1.y - sample0.y
			};
			var delta = (sample1.time - sample0.time) / 1000;
			result.x /= delta;
			result.y /= delta;
			return result;
		};

	};

	return VelocityMeter;
});