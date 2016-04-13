requirejs.config({
    baseUrl: "assets/js",
    paths: {
        jquery: 'vendor/jquery/dist/jquery.min'
    },
    packages: [
        {
            name: 'pixi',
            location: 'pixi',
            main: 'pixi.min'
        },
        'engine'
    ]
});
