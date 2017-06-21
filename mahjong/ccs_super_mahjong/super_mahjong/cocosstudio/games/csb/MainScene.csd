<GameFile>
  <PropertyGroup Name="MainScene" Type="Scene" ID="a2ee0952-26b5-49ae-8bf9-4f1d6279b798" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="35" Speed="1.0000">
        <Timeline ActionTag="953446860" Property="Scale">
          <ScaleFrame FrameIndex="0" X="1.0000" Y="1.0000">
            <EasingData Type="0" />
          </ScaleFrame>
          <ScaleFrame FrameIndex="20" X="0.5000" Y="0.5000">
            <EasingData Type="0" />
          </ScaleFrame>
          <ScaleFrame FrameIndex="35" X="1.0000" Y="1.0000">
            <EasingData Type="0" />
          </ScaleFrame>
        </Timeline>
      </Animation>
      <AnimationList>
        <AnimationInfo Name="animation0" StartIndex="0" EndIndex="35">
          <RenderColor A="255" R="147" G="112" B="219" />
        </AnimationInfo>
      </AnimationList>
      <ObjectData Name="Scene" ctype="GameNodeObjectData">
        <Size X="1136.0000" Y="640.0000" />
        <Children>
          <AbstractNodeData Name="scene01_1" ActionTag="-1513621829" Tag="3" IconVisible="False" PositionPercentXEnabled="True" PositionPercentYEnabled="True" ctype="SpriteObjectData">
            <Size X="1136.0000" Y="640.0000" />
            <AnchorPoint />
            <Position />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition />
            <PreSize X="1.0000" Y="1.0000" />
            <FileData Type="Normal" Path="games/img/nopack/scene01.jpg" Plist="" />
            <BlendFunc Src="770" Dst="771" />
          </AbstractNodeData>
          <AbstractNodeData Name="Default" ActionTag="953446860" Tag="5" IconVisible="False" LeftMargin="154.0000" RightMargin="330.0000" TopMargin="122.5000" BottomMargin="122.5000" ctype="SpriteObjectData">
            <Size X="652.0000" Y="395.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="480.0000" Y="320.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.4225" Y="0.5000" />
            <PreSize X="0.5739" Y="0.6172" />
            <FileData Type="Normal" Path="games/img/nopack/fish_shark_bj.png" Plist="" />
            <BlendFunc Src="1" Dst="771" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>