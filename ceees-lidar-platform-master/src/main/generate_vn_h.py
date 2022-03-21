
if __name__ == "__main__":
    labels = {
        "group 0": {
            "TimeStartup": "float",
            "TimeGPS": "float",
            "TimeSyncIn": "float",
            "Ypr": "float",
            "Qtn": "float",
            "AngularRate": "float",
            "Position": "float",
            "Velocity": "float",
            "Accel": "float",
            "Imu": "float",
            "MagPres": "float",
            "DeltaTheta": "float",
        },
        "group 1": {
            "InsStatus": "float",
            "SyncInCnt": "float",
            "TimeGpsPps"
            "TimeStartup": "float",
            "GpsTow": "float",
            "GpsWeek": "float",
            "TimeSyncIn": "float",
            "TimeGpsPps": "float",
            "TimeUTC": "float",
        },
        "group 2": {
            "SyncInCnt": "float",
            "ImuStatus": "float",
            "UncompMag": "float",
            "UncompAccel": "float",
            "UncompGyro": "float",
            "Temp": "float",
            "Pres": "float",
            "DeltaTheta": "float",
            "DeltaVel": "float",
            "Mag": "float",
            "Gyro": "float",
            "SensSat": "float",
            "Raw": "float",
            "UTC": "float",
            "Tow": "float",
            "Week": "float",
            "NumSats": "float",
            "Fix": "float",
            "PosLla": "float",
            "PosExec": "float",
            "VelNed": "float",
            "VelEcef": "float",
            "PosU": "float",
        },
        "group 3": {
            "VelU": "float",
            "TimeU": "float",
            "SvStat": "float",
            "VpeStatus": "float",
            "Qtn": "float",
            "DCM": "float",
            "MagNed": "float",
            "AccelNed": "float",
            "LinearAccelBody": "float",
            "LinearAccelNed": "float",
            "YprU": "float",
            "YprRate": "float",
            "StateAhrs": "float",
            "CovAhrs": "float",
            "InsStatus": "float",
            "PosLla": "float",
            "PosEcef": "float",
            "VelBody": "float",
        },
        "group 4": {
            "VelNed": "float",
            "VelEcef": "float",
            "MagEcef": "float",
            "AccelEcef": "float",
            "LinearAccelEcef": "float",
            "PosU": "float",
            "VelU": "float",
            "StateIns": "float",
            "CovIns": "float"
        }
    }

    with open("vectornav_data.h", "w") as f:
        f.write("#include \"id.h\"\n")

        vn_type_string = "#define VN_DATA_TYPE"
        for _, group in labels.items():
            for preprocessed_label, label_type in group.items():
                label = preprocessed_label.upper()
                f.write("#if defined(USE_{})\n".format(label))
                f.write("   static constexpr int {} = __id__();\n".format(label))
                f.write("   #define TYPE_{}\n".format(label))
                f.write("#else\n")
                f.write("   #define TYPE_{}\n".format(label))
                f.write("#endif\n")
                vn_type_string += " TYPE_{}".format(label)
        f.write(vn_type_string + "\n")
