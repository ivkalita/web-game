define(['backbone', 'jquery', 'events/event'], function(Backbone, $, event) {

    var Router = Backbone.Router.extend({
        initialize: function(opts) {
            this.navigation = $('#navigation');

            this.blocks = {
                gameList: $('#cont-game-list'),
                newGame: $('#cont-new-game')
            };

            event.on('newGameAdded', this.lazyNavigateTo(''), this);
        },

        routes: {
            '': 'mainPage',
            'new-game': 'newGame'
        },

        mainPage: function() {
            this.showBlock(this.blocks.gameList);

            this.activeNav('main-page');
        },

        newGame: function() {
            this.showBlock(this.blocks.newGame);

            this.activeNav('new-game');
        },

        lazyNavigateTo: function(to) {
            return function() {
                this.navigate(to, {trigger: true});
            }
        },

        showBlock: function(block) {
            for (elem in this.blocks) this.blocks[elem].hide();

            block.show()        
        },

        activeNav: function(routeName) {
            $('li', this.navigation).each(function(idx, elem) {
                $(elem).removeClass('active');
            });

            $('li[route=' + routeName + ']', this.navigation).addClass('active');
        }
    })


    return Router;
});
