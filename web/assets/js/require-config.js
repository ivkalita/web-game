requirejs.config({
    baseUrl: "assets/js",
    urlArgs: "bust=" + (new Date()).getTime(),
    paths: {
        bootstrap: 'bootstrap.min',
        backbone: 'backbone-min',
        underscore: 'underscore-min',
        jquery: 'jquery-2.1.4.min'
    },

    shim: {
        'underscore': {
            exports: '_'
        },

        'backbone': {
            deps: ['underscore', 'jquery'],
            exports: 'Backbone'
        },

        'bootstrap': {
            deps: ['jquery']
        }
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
