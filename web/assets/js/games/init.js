require(['../requirejs-config'], function() {
	require(['jquery', 'application'], function($, Application) {

		$(document).ready(function() {
			var myApplication = new Application();

			myApplication.init();
		})

	});
});
