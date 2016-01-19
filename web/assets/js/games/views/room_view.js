define(['backbone', 'models/messages', 'models/message', 'views/messages_view', 'events/event'], function(Backbone, Messages, Message, MessagesView, event) {

    var RoomView = Backbone.View.extend({
        el: $('#cont-room'),

        template: _.template($('#tpl-room').html()),

        initialize: function() {
            this.collection = new Messages();
            this.messagesView = new MessagesView({collection: this.collection});

            event.on('invalidMessage', this.invalid, this);

            this.pressedKeys = new Object();
        },

        events: {
            'click #room-send-message': 'submit',
            'keydown #room-message-text': 'keydown',
            'keyup #room-message-text': 'keyup'
        },

        render: function() {
            this.$el.html(this.template({messages: this.messagesView.render().$el.html()}));
            return this;
        },

        invalid: function(msg) {
            this.removeErrors();

            $('#room-message-text', this.el).parent().addClass('has-error');

            $('.error p', this.el).text(msg);
        },

        keydown: function(e) {
            this.pressedKeys[e.keyCode] = true;

            if (this.pressedKeys[17] && this.pressedKeys[13]) {
                this.submit();

                delete this.pressedKeys[17];
                delete this.pressedKeys[13];
            }
        },

        keyup: function(e) {
            delete this.pressedKeys[e.keyCode];
        },

        submit: function() {
            var text = $('#room-message-text', this.el).val();

            var message = new Message();

            var curDate = new Date();

            var result = message.set({
                autor: 'Игрок',
                time: beginZero(curDate.getHours()) + ':' + beginZero(curDate.getMinutes()),
                text: text
            }, {validate: true});

            if (result) {
                this.collection.add(message);
                this.collection.trigger('change');
                this.render();
            }

            $('#room-message-text').focus();
        },

        removeErrors: function() {
            $('.form-group', this.el).removeClass('has-error');
            $('.error p', this.el).text('');
        },

        clearForm: function() {
            $('.form-group ').val('');
        },
    });


    function beginZero(value) {
        return (value < 10 ? '0' + value : value);
    }


    return RoomView;
});
