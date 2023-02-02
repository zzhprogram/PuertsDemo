"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
let world = puerts_1.argv.getByName("GameInstance").GetWorld();
//JS继承一个原生类
class MyActor extends UE.Actor {
    tickCount;
    //注意，继承UE类的js类，构造函数必须大写开头
    Constructor() {
        this.PrimaryActorTick.bCanEverTick = true;
        console.log("this.Add", this.Add(33, 44));
        this.tickCount = 0;
    }
    //override 1
    ReceiveBeginPlay() {
        console.log("ReceiveBeginPlay");
        console.log("this.Add", this.Add(55, 66));
    }
    Add(a, b) {
        return a + b;
    }
    //override 2
    ReceiveTick(DeltaSeconds) {
        if (this.tickCount % 100 == 0 && this.tickCount <= 200) {
            console.log("MyActor.ReceiveTick", DeltaSeconds, this.tickCount);
        }
        ++this.tickCount;
    }
}
let cls = (0, puerts_1.makeUClass)(MyActor);
world.SpawnActor(cls, undefined, UE.ESpawnActorCollisionHandlingMethod.Undefined, undefined, undefined);
let ucls = UE.Class.Load('/Game/StarterContent/TestBlueprint.TestBlueprint_C');
//TestBlueprint是根据ucls生成的类，两者需要声明周期保持同步，慎防只拿着TestBlueprint用，ucls释放了，然后进而这个蓝图被UE给GC了
const TestBlueprint = puerts_1.blueprint.tojs(ucls);
class MyBPActor extends TestBlueprint {
    //覆盖蓝图提供的方法，此时无论是蓝图在BeginPlay的调用，以及在ts侧的调用，都会用这个新的实现
    Foo(P1, P2, P3) {
        console.log(">>>>>>>>>>>>>>>>>>>>", P1 ? P2 : P3);
    }
}
let clsBP = (0, puerts_1.makeUClass)(MyBPActor);
let bpActor2 = world.SpawnActor(clsBP, undefined, UE.ESpawnActorCollisionHandlingMethod.Undefined, undefined, undefined);
bpActor2.Foo(false, 8000, 9000);
//# sourceMappingURL=UsingMakeUClass.js.map