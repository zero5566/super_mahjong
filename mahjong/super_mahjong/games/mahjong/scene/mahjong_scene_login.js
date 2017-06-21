/**
 * 登录界面
 */
mahjong.SceneLogin = cc.Node.extend({
    ctor: function () {
        this._super();
        this.csbRoot = null;
    },

    onEnter: function () {
        this._super();
        this.csbRoot = mahjong.ccs.createNode('games/csb/MainScene.csb', true, this);
        this.addChild(this.csbRoot);
        this.csbRoot.play('animation0',true);
        cc.log('===========wwwwww走进来了.......')
        this.schedule(this.onEventStop,5);
    },

    onEventStop:function () {
        this.csbRoot.stopAllActions();

    },


    onExit: function () {
        this._super();
    }

});
