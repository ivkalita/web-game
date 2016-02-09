require(['../requirejs-config'], function() {

	require(['jquery', './scene'], function($, Scene) {
		$(document).ready(function() {
			var canvas = $('#game-screen')[0];
			new Scene(canvas, {
				width: 3000,
				height: 3000,
				borderSize: 150
			}).run();
		});
	});

});