var mahjong = mahjong || {};

cc.loader.loadJs("games/mahjong/mahjong_view.js");
cc.loader.loadJs("games/mahjong/mahjong_ccs.js");
cc.loader.loadJs("games/mahjong/mahjong_scene.js");

cc.loader.loadJs("games/mahjong/scene/mahjong_scene_login.js");
cc.loader.loadJs("games/mahjong/scene/mahjong_view_scene.js");

cc.director.runScene(new mahjong.Scene());
