/**
 * Created by shibao on 2016/6/21.
 */

/**
 * 视图 - 场景
 */
mahjong.view._Scene = mahjong.View.extend({
    ctor: function () {
        this._super();
        this.controller = mahjong.SceneLogin;
        this.child = new mahjong.SceneLogin();
        this.addChild(this.child);
    },

    onEventSwitchScene: function (controller) {
        if (this.controller == controller)
            return;
        this.removeChild(this.child);
        this.controller = controller;
        this.child = new controller();
        this.addChild(this.child);
    }
});

mahjong.view.Scene = mahjong.view.createNode.bind(null, mahjong.view._Scene);
