requirejs.config({
    baseUrl: "assets/js",
    paths: {
        jquery: 'jquery-2.1.4.min'
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
