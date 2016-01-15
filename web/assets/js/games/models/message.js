define(['backbone', 'events/event'], function(Backbone, event) {

    var Message = Backbone.Model.extend({
        initialize: function() {
            this.on('invalid', this.onInvalid);
        },

        validate: function(attrs) {
            try {
                errorIf(!attrs.text, 'Введите текст сообщения');

            } catch (exc) {
                return exc;
            }
        },
    });


    function errorIf(condition, errorMsg) {
        if (condition) {
            event.trigger('invalidMessage', errorMsg);

            throw new Error(errorMsg);
        }
    }


    return Message;
});
