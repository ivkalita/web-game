define(['backbone', 'models/messages', 'models/message', 'views/messages_view', 'events/event'], function(Backbone, Messages, Message, MessagesView, event) {

    var RoomView = Backbone.View.extend({
        el: $('#cont-room'),

        template: _.template($('#tpl-room').html()),

        initialize: function() {
            this.collection = new Messages();
            this.messagesView = new MessagesView({collection: this.collection});
            event.on('invalidMessage', this.invalid, this);
        },

        events: {
            'click #room-send-message': 'submit'
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
