define([
    'jquery', 'backbone', 'underscore'
], function($, Backbone, _) {
    var MessageModel = Backbone.Model.extend({
        defaults: {
            content: null,
            type: null
        },
        sync: function() { return false; }
    });
    MessageModel.Types = Object.freeze({SUCCESS: 'success', WARNING: 'warning', ERROR: 'danger'});
    return MessageModel;
});
