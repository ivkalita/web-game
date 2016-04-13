requirejs.config({
    baseUrl: "assets/js",
    paths: {
        jquery: 'vendor/jquery/dist/jquery.min',
		pixi: 'vendor/pixi.js/bin/pixi.min',
    },
    packages: [
        'engine'
    ]
});
