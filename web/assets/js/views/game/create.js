define(['backbone', 'underscore', '../../models/game'], function(Backbone, _, Game) {
    return Backbone.View.extend({
        template: _.template($('#tpl-matchmaking-create').html()),

        events: {
            'click button': 'submit'
        },

        render: function() {
            this.$el.html(this.template());
            return this;
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
                map: map
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
});
