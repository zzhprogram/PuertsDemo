"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const puerts_1 = require("puerts");
console.log("UMGNestedTest.ts Start");
let world = puerts_1.argv.getByName("GameInstance").GetWorld();
/*
class Init {
    private _actor = null;
    private _widget = null;

    public init(): void {
        const ucls = UE.Class.Load("/Game/StarterContent/TestBlueprint.TestBlueprint_C");
        this._actor = world.SpawnActor(ucls, undefined, UE.ESpawnActorCollisionHandlingMethod.Undefined, undefined, undefined) as UE.Actor;

        this._widget = UE.UMGManager.CreateWidget(world, mixinUMGButtonPanel.StaticClass());
        this._widget?.Button_ChangeLevel.OnClicked.Add(this._changeLevelOnclicked.bind(this));
        this._widget.AddToViewport(0);
        
    }

    private _changeLevelOnclicked(): void {
        this._actor = null;
        this._widget?.RemoveFromParent();
        this._widget?.RemoveFromViewport();
        this._widget?.Destruct();
        this._widget = null;
        umMixin();
        setTimeout(() => {
            UE.KismetSystemLibrary.CollectGarbage();
        }, 3 * 1000);

        UE.GameplayStatics.OpenLevel(world, "ChlidLevel");
    }
}

// 创建mixin的widget, 切换场景不能正常释放


// 创建的actor, 切换场景可以正常释放
const ucls = UE.Class.Load("/Game/StarterContent/TestBlueprint.TestBlueprint_C");
const actor = world.SpawnActor(ucls, undefined, UE.ESpawnActorCollisionHandlingMethod.Undefined, undefined, undefined) as UE.Actor;

 const i = new Init();
 i.init();
 */
// --------------------------------------------
//# sourceMappingURL=UMGNestedTest.js.map