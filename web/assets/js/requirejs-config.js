requirejs.config({
    paths: {
        'bootstrap': '/assets/js/bootstrap.min',
        'jquery': '/assets/js/jquery-2.1.4.min',
        'backbone': '/assets/js/backbone-min',
        'underscore': '/assets/js/underscore-min'
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
    }
});