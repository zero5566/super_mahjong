/**
 * Created by tuyoo on 2016/6/22.
 */

/**
 * 捕鱼主场景
 */
mahjong.Scene = cc.Scene.extend({

    onEnter: function () {
        this._super();

        // 初始化
        mahjong.ccs = new mahjong.CCS();

        // 界面
        this.addChild(
            mahjong.view.Scene()
        );
    },

    onExit: function () {
        this._super();

    }
});
