import { blueprint } from 'puerts';
import * as UE from 'ue'


let uclsBase = UE.Class.Load("/Game/UMGTest/UMG_Panel.UMG_Panel_C");
const MixinSuperTestBase = blueprint.tojs<typeof UE.Game.UMGTest.UMG_Panel.UMG_Panel_C>(uclsBase);

type UMGType = UE.Game.UMGTest.UMG_Panel.UMG_Panel_C;

export interface UMGButtonPanel extends UE.Game.UMGTest.UMG_Panel.UMG_Panel_C { };

export class UMGButtonPanel {
    private _ucls: UE.Class = null;
    private _widget: UE.UserWidget = null;


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

    private _createUMGNestedOnclicked(): void {
        this._ucls = UE.Class.Load("/Game/UMGTest/UMG_Main.UMG_Main_C");
        this._widget = UE.UMGManager.CreateWidget(this.GetWorld(), this._ucls) as UE.Game.UMGTest.UMG_Panel.UMG_Panel_C;
        this._widget.AddToViewport(0);
    }

    private _createUMGAloneOnclicked(): void {
        this._ucls = UE.Class.Load("/Game/UMGTest/UMG_Alone.UMG_Alone_C");
        this._widget = UE.UMGManager.CreateWidget(this.GetWorld(), this._ucls) as UE.Game.UMGTest.UMG_Panel.UMG_Panel_C;
        this._widget.AddToViewport(0);
    }
}

export const mixinUMGButtonPanel = blueprint.mixin(MixinSuperTestBase, UMGButtonPanel);

// 取消mixin操作
export function umMixin(): void {
    console.log("umMixin");
    let ucls = UE.Class.Load("/Game/UMGTest/UMG_Panel.UMG_Panel_C");
    let uclsObj = blueprint.tojs<typeof UE.Game.UMGTest.UMG_Panel.UMG_Panel_C>(ucls);
    blueprint.unmixin(uclsObj);
}
