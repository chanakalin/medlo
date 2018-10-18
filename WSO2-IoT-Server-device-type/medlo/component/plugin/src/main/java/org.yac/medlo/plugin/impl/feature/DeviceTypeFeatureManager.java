/*
 * Copyright (c) 2016, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.yac.medlo.plugin.impl.feature;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.wso2.carbon.device.mgt.common.DeviceManagementException;
import org.wso2.carbon.device.mgt.common.Feature;
import org.wso2.carbon.device.mgt.common.FeatureManager;

import java.util.List;

/**
 * Device type specific feature management server
 */
public class DeviceTypeFeatureManager implements FeatureManager {

    private static final String METHOD = "method";
    private static final String URI = "uri";
    private static final String CONTENT_TYPE = "contentType";
    private static final String PATH_PARAMS = "pathParams";
    private static final String QUERY_PARAMS = "queryParams";
    private static final String FORM_PARAMS = "formParams";
    private static Feature feature = new Feature();

    public DeviceTypeFeatureManager() {
        feature.setCode("configranges");
        feature.setName("Set temperature humidity ranges");
        feature.setDescription("Set maximum and minimum values for temperature and humidity");

        Map<String, Object> apiParams = new HashMap<>();
        apiParams.put(METHOD, "POST");
        apiParams.put(URI, "/medlonode/device/{deviceId}/configranges");
        List<String> pathParams = new ArrayList<>();
        List<String> queryParams = new ArrayList<>();
        List<String> formParams = new ArrayList<>();
        pathParams.add("deviceId");
        apiParams.put(PATH_PARAMS, pathParams);
        queryParams.add("max-temperature");
        queryParams.add("min-temperature");
        queryParams.add("max-humidity");
        queryParams.add("min-humidity");
        apiParams.put(QUERY_PARAMS, queryParams);
        apiParams.put(FORM_PARAMS, formParams);
        List<Feature.MetadataEntry> metadataEntries = new ArrayList<>();
        Feature.MetadataEntry metadataEntry = new Feature.MetadataEntry();
        metadataEntry.setId(-1);
        metadataEntry.setValue(apiParams);
        metadataEntries.add(metadataEntry);
        feature.setMetadataEntries(metadataEntries);
    }

    @Override
    public boolean addFeature(Feature feature) throws DeviceManagementException {
        return false;
    }

    @Override
    public boolean addFeatures(List<Feature> features) throws DeviceManagementException {
        return false;
    }

    @Override
    public Feature getFeature(String name) throws DeviceManagementException {
        return feature;
    }

    @Override
    public List<Feature> getFeatures() throws DeviceManagementException {
        List<Feature> features = new ArrayList<>();
        features.add(feature);
        return features;
    }

    @Override
    public boolean removeFeature(String name) throws DeviceManagementException {
        return false;
    }

    @Override
    public boolean addSupportedFeaturesToDB() throws DeviceManagementException {
        return false;
    }
}
