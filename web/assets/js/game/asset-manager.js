define(['jquery', './webgl'], function($, Webgl) {

	return {
		create: function(gl) {

			var loadTexture2D = function(url) {
				var task = $.Deferred();
				var image = new Image();

				var unbindEvents = function() {
					image.onload = image.onerror = image.onabort = null;
				};

				image.onload = function() {
					unbindEvents();
					task.resolve(Webgl.createTexture2D(gl, image));
				};

				image.onerror = image.onabort = function() {
					unbindEvents();
					task.reject({
						url: url,
						message: "Can't load"
					});
				};

				image.src = url;

				return task.promise();
			};

			var loadShader = function(url, type) {
				var task = $.Deferred();

				$.ajax({
					url: url,
					dataType: 'text',
				}).then(function(code) {
					try {
						task.resolve(Webgl.createShader(gl, type, code));
					} catch (e) {
						task.reject({
							url: url,
							message: e.message
						});
					}
				}).fail(function(req, status) {
					task.reject({
						url: url,
						message: "Can't load"
					})
				});

				return task.promise();
			};

			var loadProgram = function(url) {
				var task = $.Deferred();

				loadJson(url).then(function(json) {
					return $.when(
						loadShader(json.vs, gl.VERTEX_SHADER),
						loadShader(json.fs, gl.FRAGMENT_SHADER)
					);
				}).then(function(vs, fs) {
					try {
						task.resolve(Webgl.createProgram(gl, vs, fs));
						vs.dispose();
						fs.dispose();
					} catch (e) {
						task.reject({
							url: url,
							message: e.message
						});
					}
				}).fail(function(err) {
					task.reject(err);
				})

				return task.promise();
			};

			var loadJson = function(url) {
				var task = $.Deferred();

				$.ajax({
					url: url,
					dataType: 'json',
				}).then(function(json) {
					task.resolve(json);
				}).fail(function(req, status) {
					task.reject({
						url: url,
						message: "Can't load"
					});
				});

				return task.promise();
			};

			var load = function(assetList) {
				var result = {};
				var promises = $.when();
				var ioStack = [
					{
						input: assetList.textures,
						output: result.textures = {},
						load: loadTexture2D
					},
					{
						input: assetList.programs,
						output: result.programs = {},
						load: loadProgram
					},
					{
						input: assetList.jsons,
						output: result.jsons = {},
						load: loadJson
					}
				];

				var enqueue = function(url, load, outputBlock, id) {
					promises = promises.then(function() {
						return load(url).then(function(result) {
							outputBlock[id] = result;
						});
					});
				};

				while (ioStack.length) {
					var io = ioStack.pop();
					if (io.input === undefined) {
						continue;
					}
					$.each(io.input, function(id, member) {
						if (typeof (member) === 'string') {
							enqueue(member, io.load, io.output, id);
						} else {
							ioStack.push({
								input: member,
								output: io.output[id] = {},
								load: io.load
							});
						}
					});
				}

				return $.Deferred(function(task) {
					promises.then(function() {
						task.resolve(result);
					}).fail(function(err) {
						task.reject(err);
					});
				}).promise();
			};

			return {
				loadTexture2D: loadTexture2D,
				loadProgram: loadProgram,
				loadJson: loadJson,
				load: load
			}
		}
	}
});