define(['backbone', 'models/game', 'events/event'], function(Backbone, Game, event) {

    var FormView = Backbone.View.extend({
        el: $('#cont-new-game'),

        template: _.template($('#tpl-new-game').html()),

        initialize: function() {
            event.on('invalidGame', this.invalid, this);
        },

        events: {
            'click #new-game-submit': 'submit'
        },

        render: function() {
            this.$el.html(this.template());
            return this;
        },

        invalid: function(field, msg) {
            this.removeErrors();

            $('#new-game-' + field, this.el).parent().addClass('has-error');

            $('.error p, this.el').text(msg);
        },

        submit: function() {
            var name = $('#new-game-name', this.el).val();
            var mod = $('#new-game-mod', this.el).val();
            var map = $('#new-game-map', this.el).val();
            var maxPlayers = $('#new-game-max-players', this.el).val();

            var newGame = new Game();

            var result = newGame.set({
                name: name, 
                owner: 'Пользователь', 
                curNumPlayers: 0, 
                maxNumPlayers: 
                maxPlayers, 
                mod: mod, 
                map: map,
            }, {validate: true});

            if (result) {
                this.collection.add(newGame);
                this.collection.trigger('change');
                this.clearForm();
                
                event.trigger('newGameAdded');
            }
        },

        removeErrors: function() {
            $('.form-group', this.el).removeClass('has-error');
            $('.error p', this.el).text('');
        },

        clearForm: function() {
            this.removeErrors();
            $('#new-game-name').val('');
        }
    });


    return FormView;
});
