define(['backbone', 'events/event'], function(Backbone, event) {

    var Game = Backbone.Model.extend({
        initialize: function() {
            this.on('invalid', this.onInvalid);
        },

        validate: function(attrs) {
            try {
                errorIf(!attrs.name, 'name', 'Не указано имя игры');
                errorIf(attrs.name.length < 4, 'name', 'Имя игры не может быть короче, чем 4 символа');

            } catch (exc) {
                return exc;
            }
        },

        onInvalid: function(model, error) {},
    });


    function errorIf(condition, field, errorMsg) {
        if (condition) {
            event.trigger('invalidGame', field, errorMsg);

            throw new Error(errorMsg);
        }
    }


    return Game;
});
