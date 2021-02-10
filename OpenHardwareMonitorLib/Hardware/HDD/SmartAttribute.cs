/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2011-2015 Michael Möller <mmoeller@openhardwaremonitor.org>
  Copyright (C) 2011 Roland Reinl <roland-reinl@gmx.de>
	
*/

using System;
using System.Collections.Generic;
using OpenHardwareMonitor.Collections;

namespace OpenHardwareMonitor.Hardware.HDD {
  internal class SmartAttribute {

    private RawValueConversion rawValueConversion;

    /// <summary>
    /// Initializes a new instance of the <see cref="SmartAttribute"/> class.
    /// </summary>
    /// <param name="identifier">The SMART identifier of the attribute.</param>
    /// <param name="name">The name of the attribute.</param>
    public SmartAttribute(byte identifier, string name) : 
      this(identifier, name, null, null, 0, null) { }

    /// <summary>
    /// Initializes a new instance of the <see cref="SmartAttribute"/> class.
    /// </summary>
    /// <param name="identifier">The SMART identifier of the attribute.</param>
    /// <param name="name">The name of the attribute.</param>
    /// <param name="rawValueConversion">A delegate for converting the raw byte 
    /// array into a value (or null to use the attribute value).</param>
    public SmartAttribute(byte identifier, string name,
      RawValueConversion rawValueConversion) :
      this(identifier, name, rawValueConversion, null, 0, null) { }

    /// <summary>
    /// Initializes a new instance of the <see cref="SmartAttribute"/> class.
    /// </summary>
    /// <param name="identifier">The SMART identifier of the attribute.</param>
    /// <param name="name">The name of the attribute.</param>
    /// <param name="rawValueConversion">A delegate for converting the raw byte 
    /// array into a value (or null to use the attribute value).</param>
    /// <param name="sensorType">Type of the sensor or null if no sensor is to 
    /// be created.</param>
    /// <param name="sensorChannel">If there exists more than one attribute with 
    /// the same sensor channel and type, then a sensor is created only for the  
    /// first attribute.</param>
    /// <param name="sensorName">The name to be used for the sensor, or null if 
    /// no sensor is created.</param>
    /// <param name="defaultHiddenSensor">True to hide the sensor initially.</param>
    /// <param name="parameterDescriptions">Description for the parameters of the sensor 
    /// (or null).</param>
    public SmartAttribute(byte identifier, string name,
      RawValueConversion rawValueConversion, SensorType? sensorType, 
      int sensorChannel, string sensorName, bool defaultHiddenSensor = false,
      ParameterDescription[] parameterDescriptions = null) 
    {
      this.Identifier = identifier;
      this.Name = name;
      this.rawValueConversion = rawValueConversion;
      this.SensorType = sensorType;
      this.SensorChannel = sensorChannel;
      this.SensorName = sensorName;
      this.DefaultHiddenSensor = defaultHiddenSensor;
      this.ParameterDescriptions = parameterDescriptions;
    }

    /// <summary>
    /// Gets the SMART identifier.
    /// </summary>
    public byte Identifier { get; private set; }

    public string Name { get; private set; }

    public SensorType? SensorType { get; private set; }

    public int SensorChannel { get; private set; }

    public string SensorName { get; private set; }

    public bool DefaultHiddenSensor { get; private set; }

    public ParameterDescription[] ParameterDescriptions { get; private set; }

    public bool HasRawValueConversion {
      get {
        return rawValueConversion != null;
      }
    }

    public float ConvertValue(DriveAttributeValue value, 
      IReadOnlyArray<IParameter> parameters) 
    {
      if (rawValueConversion == null) {
        return value.AttrValue;
      } else {
        return rawValueConversion(value.RawValue, value.AttrValue, parameters);
      }
    }

    public delegate float RawValueConversion(byte[] rawValue, byte value,
      IReadOnlyArray<IParameter> parameters);
  }
}
