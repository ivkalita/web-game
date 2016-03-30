requirejs.config({
    paths: {
        'bootstrap': '../bootstrap.min',
        'jquery': '../jquery-2.1.4.min',
        'backbone': '../backbone-min',
        'underscore': '../underscore-min'
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
})


require(['jquery', 'application'], function($, Application) {

    $(document).ready(function() {
        var myApplication = new Application();

        myApplication.init();
    })

});
