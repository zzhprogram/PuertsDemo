"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.umMixin = exports.mixinUMGButtonPanel = exports.UMGButtonPanel = void 0;
const puerts_1 = require("puerts");
const UE = require("ue");
let uclsBase = UE.Class.Load("/Game/UMGTest/UMG_Panel.UMG_Panel_C");
const MixinSuperTestBase = puerts_1.blueprint.tojs(uclsBase);
;
class UMGButtonPanel {
    _ucls = null;
    _widget = null;
    Construct() {
        console.log("UMGButtonPanel constructor");
        this.Button_Click_WithNested.OnClicked.Add(this._createUMGNestedOnclicked.bind(this));
        this.Button_Click_WihtOutNested.OnClicked.Add(this._createUMGAloneOnclicked.bind(this));
    }
    Desstruct() {
        console.log("UMGButtonPanel Desstruct");
        this.Button_Click_WithNested.OnClicked.Clear();
        this.Button_Click_WihtOutNested.OnClicked.Clear();
        this.Button_ChangeLevel.OnClicked.Clear();
    }
    _createUMGNestedOnclicked() {
        this._ucls = UE.Class.Load("/Game/UMGTest/UMG_Main.UMG_Main_C");
        this._widget = UE.UMGManager.CreateWidget(this.GetWorld(), this._ucls);
        this._widget.AddToViewport(0);
    }
    _createUMGAloneOnclicked() {
        this._ucls = UE.Class.Load("/Game/UMGTest/UMG_Alone.UMG_Alone_C");
        this._widget = UE.UMGManager.CreateWidget(this.GetWorld(), this._ucls);
        this._widget.AddToViewport(0);
    }
}
exports.UMGButtonPanel = UMGButtonPanel;
exports.mixinUMGButtonPanel = puerts_1.blueprint.mixin(MixinSuperTestBase, UMGButtonPanel);
// 取消mixin操作
function umMixin() {
    console.log("umMixin");
    let ucls = UE.Class.Load("/Game/UMGTest/UMG_Panel.UMG_Panel_C");
    let uclsObj = puerts_1.blueprint.tojs(ucls);
    puerts_1.blueprint.unmixin(uclsObj);
}
exports.umMixin = umMixin;
//# sourceMappingURL=UMGButtonPane.js.map