#include "radapter/radapter.hpp"
#include "common.hpp"
#include "sl_lidar.h"
#include "sl_lidar_driver.h"
#include <qserialportinfo.h>

using namespace radapter;
using namespace sl;

struct LidarConfig {
    QString port;

    WithDefault<unsigned> baud = 115200;
};

RAD_DESCRIBE(LidarConfig) {
    RAD_MEMBER(port);
    RAD_MEMBER(baud);
}

static string_view PrintSlErr(sl_result err) {
    if (err == SL_RESULT_ALREADY_DONE) return "Already done";
    switch (err) {
    case SL_RESULT_INVALID_DATA: return "SL_RESULT_INVALID_DATA";
    case SL_RESULT_OPERATION_FAIL: return "SL_RESULT_OPERATION_FAIL";
    case SL_RESULT_OPERATION_TIMEOUT: return "SL_RESULT_OPERATION_TIMEOUT";
    case SL_RESULT_OPERATION_STOP: return "SL_RESULT_OPERATION_STOP";
    case SL_RESULT_OPERATION_NOT_SUPPORT: return "SL_RESULT_OPERATION_NOT_SUPPORT";
    case SL_RESULT_FORMAT_NOT_SUPPORT: return "SL_RESULT_FORMAT_NOT_SUPPORT";
    case SL_RESULT_INSUFFICIENT_MEMORY: return "SL_RESULT_INSUFFICIENT_MEMORY";
    default: return err & SL_RESULT_FAIL_BIT ? "Unknown error" : "No error";
    }
}


class RadapterRPLidar final : public radapter::Worker {
    Q_OBJECT

    LidarConfig config;
    std::unique_ptr<IChannel> _chan;
    std::unique_ptr<ILidarDriver> _driver;
    sl_lidar_response_device_info_t deviceInfo;
public:
    ~RadapterRPLidar() override {
        _driver.reset();
        _chan.reset();
    }
    RadapterRPLidar(QVariantList const& args, Instance* inst) :
        Worker(inst, "lidar")
    {
        Parse(config, args.value(0));
        Result<IChannel*> channel = createSerialPortChannel(config.port.toStdString().c_str(), config.baud);
        if (!channel) {
            throw Err("Could not open port: {} (@{}) => {}",
                      config.port, config.baud, PrintSlErr(channel.err));
        }
        ILidarDriver* lidar = *createLidarDriver();
        if (auto res = lidar->connect(*channel); !SL_IS_OK(res)) {
            throw Err("Error connecting: {}", PrintSlErr(res));
        }
        if (auto res = lidar->getDeviceInfo(deviceInfo); !SL_IS_OK(res)) {
            throw Err("Error getting info: {}", PrintSlErr(res));
        }
        Info("Model: {}, FW: {}.{}, HW: {}",
             deviceInfo.model,
             deviceInfo.firmware_version >> 8,
             deviceInfo.firmware_version & 0xffu,
             deviceInfo.hardware_version);
    }
    void OnMsg(QVariant const& msg) override {

    }
};



RADAPTER_PLUGIN(RadapterRPLidar, "radapter.plugins.RPLidar")
#include "radapter_rplidar.moc"
