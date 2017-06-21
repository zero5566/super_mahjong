/**
 * 名字空间
 */
mahjong.view = {};
mahjong.view.createNode = function (controller) {
    var args = [];
    var children = [];
    for (var i = 1; i < arguments.length; i++) {
        var obj = arguments[i];
        if (mahjong.View.prototype.isPrototypeOf(obj)) {
            children.push(obj);
        } else {
            args.push(obj);
        }
    }

    var node = Object.create(controller.prototype);
    controller.apply(node, args);

    for (i = 0; i < children.length; i++) {
        node.addChild(children[i]);
    }

    return node;
};

/**
 * 场景层级基类
 */
mahjong.View = cc.Node.extend({});

/**
 * 层级定义
 */
mahjong.View.DEFAULT = 0;     // 默认层级
