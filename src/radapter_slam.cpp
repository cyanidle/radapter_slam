#include "radapter/radapter.hpp"
#include "common.hpp"


using namespace radapter;



class RadapterSlam final : public radapter::Worker {
    Q_OBJECT
public:
    RadapterSlam(QVariantList const& args, Instance* inst) :
        Worker(inst, "slam")
    {

    }
    void OnMsg(QVariant const& msg) override {

    }
};



RADAPTER_PLUGIN(RadapterSlam, "radapter.plugins.Slam")
#include "radapter_slam.moc"
