
/**
 * Cocos Studio辅助类
 */
mahjong.CCS = cc.Class.extend({
    /**
     * 加载CSB文件
     * @param isFullSize 是否全屏大小：对于小UI为false，对于适应屏幕分辨率的UI为true
     * @param controller controller对象
     * @param callback load完成后回调
     * @param target 回调对象
     * @returns {*}
     */
    loadCSB: function (filename, isFullSize, controller, callback, target) {
        var node = this.createNode(filename, isFullSize, controller, callback, target);
        var action = this.createTimeline(filename);
        return {node: node, action: action};
    },

    /**
     * 创建CSB节点
     * @returns {cc.Node}
     */
    createNode: function (filename, isFullSize, controller, callback, target) {
        var node = _createNode(filename, isFullSize);
        cc.log('=========wwwwwwweeee=======' + filename+'===============' + JSON.stringify(node))
        if (controller)
            this.bindController(node, controller);
        if (callback)
            callback.call(target || null);
        node.csbFile = filename;
        node.play = mahjong.ccs._csbNodeUtils.play;
        node.play2 = mahjong.ccs._csbNodeUtils.play2;
        node.play3 = mahjong.ccs._csbNodeUtils.play3;
        node.sequence = mahjong.ccs._csbNodeUtils.sequence;
        node.gotoFrameAndPlay = mahjong.ccs._csbNodeUtils.gotoFrameAndPlay;
        node.gotoFrame = mahjong.ccs._csbNodeUtils.gotoFrame;
        node.gotoAnimationBegin = mahjong.ccs._csbNodeUtils.gotoAnimationBegin;
        node.gotoAnimationEnd = mahjong.ccs._csbNodeUtils.gotoAnimationEnd;
        node.setAnimationEndCallFunc = mahjong.ccs._csbNodeUtils.setAnimationEndCallFunc;
        node.setAnimationEndRemove = mahjong.ccs._csbNodeUtils.setAnimationEndRemove;
        return node;
    },

    /**
     * 创建时间线
     * @returns {cc.ActionTimeline}
     */
    createTimeline: function (filename) {
        return _createTimeline(filename);
    },

    /**
     * 删除时间线
     */
    removeTimeline: function (filename) {
        return _removeTimeline(filename);
    },

    /**
     * CSB节点与Controller绑定
     * 适应于两种情况：
     * 1. controller为csbNode的父节点：csbNode的生命周期跟随controller
     * 2. controller非csbNode的父节点：csbNode的生命周期比controller长，controller如果
     *    绑定了自己的成员函数给csbNode的按钮，在退出时需要进行解绑(非父节点尽量别绑函数)
     * @param root csb根节点
     * @param controller controller对象
     */
    bindController: function (root, controller) {
        if (!root)
            return;

        var outletName = root.getName();
        if (outletName in controller && typeof controller[outletName] !== 'function')
            controller[outletName] = root;

        if (typeof root.getCallbackName === 'function') {
            var callbackName = root.getCallbackName();
            if (callbackName && controller[callbackName]) {
                controller[outletName] = root;
                root.addTouchEventListener(controller[callbackName], controller);
            }
        }

        var arrayRootChildren = root.getChildren();
        var length = arrayRootChildren.length;
        for (var i = 0; i < length; i++) {
            var child = arrayRootChildren[i];
            this.bindController(child, controller);
        }
    },

    /**
     * 查找子节点
     * @param root
     * @param name
     * @returns {cc.Node}
     */
    seekNodeByName: function (root, name) {
        if (!root)
            return null;
        if (root.getName() === name)
            return root;
        var arrayRootChildren = root.getChildren();
        var length = arrayRootChildren.length;
        for (var i = 0; i < length; i++) {
            var child = arrayRootChildren[i];
            var res = this.seekNodeByName(child, name);
            if (res !== null)
                return res;
        }
        return null;
    },

    /**
     * 辅助函数
     */
    _csbNodeUtils: {
        /**
         * 播放指定动画
         * @param animationName
         * @param loop
         */
        play: function (animationName, loop) {
            var action = _createTimeline(this.csbFile);
            this.runningAction = action;
            action.play(animationName, loop);
            this.runAction(action);
        },

        /**
         * 顺序播放2个动画
         * @param act1
         * @param act2
         * @param loopLast 是否循环最后一个动画
         */
        play2: function (act1, act2, loopLast) {
            var action = _createTimeline(this.csbFile);
            this.runningAction = action;
            action.setAnimationEndCallFunc(act1, function () {
                this.play(act2, loopLast);
            });
            action.play(act1, false);
            this.runAction(action);
        },

        /**
         * 顺序播放3个动画
         * @param act1
         * @param act2
         * @param act3
         * @param loopLast 是否循环最后一个动画
         */
        play3: function (act1, act2, act3, loopLast) {
            var action = _createTimeline(this.csbFile);
            this.runningAction = action;
            action.setAnimationEndCallFunc(act2, function () {
                this.play(act3, loopLast);
            });
            action.setAnimationEndCallFunc(act1, function () {
                this.play(act2, false);
            });
            action.play(act1, false);
            this.runAction(action);
        },

        /**
         * 顺序播放：sequence(anim1, anim2, anim3, ..., true);
         * 当几个动画是连续的时间线时
         */
        sequence: function () {
            if (arguments.length < 3)
                return;
            var loop = arguments[arguments.length - 1];
            var action = _createTimeline(this.csbFile);
            var startAnimationInfo = action.getAnimationInfo(arguments[0]);
            var endAnimationInfo = action.getAnimationInfo(arguments[arguments.length - 2]);
            var startIndex = endAnimationInfo.startIndex;
            var endIndex = endAnimationInfo.endIndex;
            var currentFrameIndex = startAnimationInfo.startIndex;
            this.runningAction = action;
            action.gotoFrameAndPlay(startIndex, endIndex, currentFrameIndex, loop);
            this.runAction(action);
        },

        /**
         * 跳转到指定帧，然后播放
         * @param startIndex
         * @param endIndex
         * @param currentFrameIndex
         * @param loop
         */
        gotoFrameAndPlay: function (startIndex, endIndex, currentFrameIndex, loop) {
            var action = _createTimeline(this.csbFile);
            this.runningAction = action;
            action.gotoFrameAndPlay.apply(action, arguments);
            this.runAction(action);
        },

        /**
         * 设置CSB界面为指定帧
         * @param frame
         */
        gotoFrame: function (frame) {
            var action = _createTimeline(this.csbFile);
            this.runAction(action);

            action.gotoFrameAndPause(frame);
            this.stopAllActions();
        },

        /**
         * 设置CSB界面为指定动画的开始
         * @param animationName
         */
        gotoAnimationBegin: function (animationName) {
            var action = _createTimeline(this.csbFile);
            action.play(animationName, false);
            this.runAction(action);

            var startFrame = action.getStartFrame();
            action.gotoFrameAndPause(startFrame);
            this.stopAllActions();
        },

        /**
         * 设置CSB界面为指定动画的结束
         * @param animationName
         */
        gotoAnimationEnd: function (animationName) {
            var action = _createTimeline(this.csbFile);
            action.play(animationName, false);
            this.runAction(action);

            var endFrame = action.getEndFrame();
            action.gotoFrameAndPause(endFrame);
            this.stopAllActions();
        },

        /**
         * 设置动画结束移除节点
         * @param animationName
         */
        setAnimationEndRemove: function (animationName) {
            if (this.runningAction) {
                var node = this;
                this.runningAction.setAnimationEndCallFunc(animationName, function () {
                    node.removeFromParent();
                });
            }
        },

        /**
         * 设置动画结束回调
         * @param animationName
         * @param func
         */
        setAnimationEndCallFunc: function (animationName, func) {
            if (this.runningAction)
                this.runningAction.setAnimationEndCallFunc(animationName, func);
        }
    }
});
