define(['jquery', 'backbone', '../models/user'], function($, Backbone, User) {
    return Backbone.Collection.extend({
        model: User
    });
});
